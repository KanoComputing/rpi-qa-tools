/**
 * qa-findimage.cpp
 *
 * Copyright (C) 2016-2019 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * A console tool to search an image contained in another image using OpenCV matchTemplate algorithm.
 *
 * Read the official explanation to understand how this works:
 *     http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html
 *
 */

#include <docopt/docopt.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>

#include <iostream>
#include <memory>
#include <string>

#include "qa-findimage/rpi-screenshot.h"

using cv::Mat;
using cv::Point;

// function prototypes
void MatchingMethod(int, void*, const std::string &output_image);

#define DEFAULT_ACCURACY_RATIO   0.55f

// Global Variables
Mat img;               // img is the source image
Mat templ;             // templ is the image we want to find
Mat result;            // the result of finding templ in img
double accuracy_ratio;


// TODO: command line
bool verbose = false;
bool ui = false;

// Provide this literal as the source image to find image on the RPi screen
std::string CMDLINE_DISPMANX_SOURCE = "dispmanx";
std::string CMDLINE_SHOW_UI = "ui";



static const char USAGE[] =
  R"(qa-findimage is a tool that locates assets on the screen and images. It will dump a Json object with finding details.

Usage:
    qa-findimage <source_image> <asset_image> [--output=target_image] [--verbose] [--accuracy=n]

Arguments:
    <source_image> Filename of the source image, where you want to locate the given asset.
                   pass 'dispmanx' on this parameter to take a RaspberryPI screenshot as the source image.

    <asset_image>  Filename of an image that you want to locate on the source_image.
                   Normally this will be a smaller image, an icon or other asset contained on the source image.
                   The resolution does not need to be exactly the same as the one on the source image.

    [--output=target_image]
                   Optionally you can dump the finding on a .png file, this will be the source image,
                   with a rectangle to mark the asset that has been found - in green - or otherwise
                   in red, indicating that it was not correctly found.

                   Pass 'ui' to popup an interactive X11 window to show the results,
                   instead of saving it as a target_image.
Options:

    -v --verbose
                   Explain the steps taken through the process.

    -a --accuracy
                   A float number from 0.0 to 1.0 indicating the amount of expected accuracy to decide
                   when the asset is correctly found. Use this option on rare cases where
                   a false negative is detected.

    -h, --help
        Show this message.

Examples:
    qa-findimage dispmanx /usr/share/icons/desktop.png
    { 'found': true, 'x': 452, 'y': 228, 'width': 86, 'height': 88, 'minVal': -0.334045, 'maxVal': 0.868913 }

    qa-findimage desktop-screenshot.png /usr/share/icons/desktop.png ui
    { 'found': true, 'x': 452, 'y': 228, 'width': 86, 'height': 88, 'minVal': -0.334045, 'maxVal': 0.868913 }

    The ui option will popup a X11 window displaying desktop-screenshot.png with a green rectangle
    sorrounding the desktop.png icon.

    qa-findimage desktop-screenshot.png /usr/share/icons/desktop.png results.png
    { 'found': true, 'x': 452, 'y': 228, 'width': 86, 'height': 88, 'minVal': -0.334045, 'maxVal': 0.868913 }

    Same as above, but instead of popping up a window, the image will be saved at results.png.)";

int main(int argc, char** argv)
{
    std::string source_image, image_to_find, output_image, accuracy;
    std::shared_ptr<void> rpi_screen = NULL;
    bool verbose=false;

    // Collect docopt command line options
    std::map<std::string, docopt::value> args =
        docopt::docopt(USAGE, {argv + 1, argv + argc});

    verbose = args["--verbose"].asBool();
    source_image = args["<source_image>"].asString();
    image_to_find = args["<asset_image>"].asString();

    if (args["--accuracy"].isString()) {
        accuracy_ratio = atof (args["--accuracy"].asString().c_str());
    }
    else {
        accuracy_ratio = DEFAULT_ACCURACY_RATIO;
    }

    if (args["--output"].isString()) {
        output_image = args["--output"].asString();
    }

    if (verbose) {
        for(auto const& arg : args) {
            std::cout << arg.first << ": " << arg.second << std::endl;
        }
        
        std::cout << "assigned options " << source_image << std::endl;
        std::cout << " source image: " << source_image << std::endl;
        std::cout << " asset image: " << image_to_find << std::endl;
        std::cout << " output image: " << image_to_find << std::endl;
        std::cout << " accuracy: " << accuracy_ratio << std::endl;
    }

    // output image to graphically explain the match
    if (output_image == CMDLINE_SHOW_UI) {
        // show it on the screen directly in a X11 window
        ui = true;
    }

    // the magic name "rpi" for source image means a RaspberryPI screenshot
    if (CMDLINE_DISPMANX_SOURCE == source_image) {
        int rows = 0;
        int cols = 0;
        rpi_screen = get_rpi_screenshot(verbose, &cols, &rows);

        if (!rpi_screen) {
            std::cout << "Error: could not take a dispmanx screenshot\n";
            return 1;
        } else {
            // CV_8UC3 means 8-bit unsigned integer matrix, with 3 bytes per
            // channel.
            img = Mat(rows, cols, CV_8UC3, rpi_screen.get());
            if (!img.data) {
                std::cout << "Error: could not transform dispmanx data\n";
                return 1;
            } else {
                // Transform screenshot to use Opencv's default BGR colorspace.
                cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
            }
        }
    } else {
        // load images, sanity checks
        img = cv::imread(source_image, 1);
        if (!img.data) {
            std::cout << "Error: could not open source image file: "
                      << source_image << std::endl;
            return 1;
        }
    }

    // the image we want to find
    templ = cv::imread(image_to_find, 1);
    if (!templ.data) {
        std::cout << "Error: could not open image to find file: "
                  << image_to_find << std::endl;
        return 1;
    }

    MatchingMethod(0, 0, output_image);
    return 0;
}

bool findImage (Mat *result, Mat *img, Mat *templ, int match_method, double accuracy_ratio,
                double *minVal, double *maxVal, Point *minLoc, Point *maxLoc, Point *matchLoc)
{
    double accuracy_val=0;

    // Create the result matrix
    int result_cols = img->cols - templ->cols + 1;
    int result_rows = img->rows - templ->rows + 1;

    // The actual image search happens here
    result->create(result_rows, result_cols, CV_32FC1);
    cv::normalize(*result, *result, 0, 1, cv::NORM_MINMAX, -1, Mat());
    cv::matchTemplate(*img, *templ, *result, match_method);
    cv::minMaxLoc(*result, minVal, maxVal, minLoc, maxLoc, Mat());

    // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all
    // the other methods, the higher the better
    accuracy_val = *maxVal;
    matchLoc->x = maxLoc->x;
    matchLoc->y = maxLoc->y;

    if (verbose) {
        std::cout << "findImage iteration accuracy_val=" << accuracy_val << " accuracy_ratio=" << accuracy_ratio << std::endl;
    }

    // return a json to the console with the finding
    return (accuracy_val < accuracy_ratio ? false : true);
}



void MatchingMethod(__attribute__((unused)) int count,
                    __attribute__((unused)) void *userdata,
                    const std::string &output_image)
{
    // Choose an alternative mathing method here
    // CV_TM_SQDIFF, CV_TM_SQDIFF_NORMED, CV_TM_CCORR
    // CV_TM_CCORR_NORMED, CV_TM_CCOEFF, CV_TM_CCOEFF_NORMED

    int match_method = CV_TM_CCOEFF_NORMED;
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;
    bool found;

    // Source image to display
    Mat img_display;
    img.copyTo(img_display);

    // Try to find the image
    found = findImage(&result, &img, &templ, match_method, accuracy_ratio, &minVal, &maxVal,
                      &minLoc, &maxLoc, &matchLoc);
    if (!found && templ.cols) {

        // If the resized asset would become larger than the source image
        // there is no point on trying to find it
        if ((templ.cols * 1.25 < img.cols) && (templ.rows * 1.25 < img.rows)) {
            // try again but resizing the image UP to find to a smaller size
            cv::resize(templ, templ, cv::Size(templ.cols * 1.25, templ.rows * 1.25), 0, 0, CV_INTER_LINEAR);
            found = findImage(&result, &img, &templ, match_method, accuracy_ratio, &minVal, &maxVal,
                              &minLoc, &maxLoc, &matchLoc);
            if (!found) {
                // try again but resizing the image DOWN to find to a smaller size
                cv::resize(templ, templ, cv::Size(templ.cols * 0.40, templ.rows * 0.40), 0, 0, CV_INTER_LINEAR);
                found = findImage(&result, &img, &templ, match_method, accuracy_ratio, &minVal, &maxVal,
                                  &minLoc, &maxLoc, &matchLoc);
            }
        }
    }
    
    std::cout << "{ "
              <<     "\"found\": " << (found ? "true" : "false") << ", "
              <<     "\"x\": " << matchLoc.x << ", "
              <<     "\"y\": " << matchLoc.y << ", "
              <<     "\"width\": " << templ.cols << ", "
              <<     "\"height\": " << templ.rows << ", "
              <<     "\"minVal\": " << minVal << ", "
              <<     "\"maxVal\": " << maxVal << " "
              << "}\n";

    // If output image is requested, draw a rectangle to show where we found it
    // In green means it's a positive, in red means not found.
    if (output_image.length()) {
        // Draw a rectangle on the area of interest
        cv::Scalar rgb_green = cv::Scalar(0.0, 255, 0.0);
        cv::Scalar rgb_red = cv::Scalar(0.0, 0.0, 255);

        Point match_coord(matchLoc.x + templ.cols, matchLoc.y + templ.rows);
        cv::rectangle(img_display, matchLoc, match_coord,
                  found ? rgb_green : rgb_red, 2, 8, 0);

        // Either save an image to show finding, or open up a X11 window
        if (output_image == CMDLINE_SHOW_UI) {
            std::string window_name =
                std::string("FindImage: ") + (found ? "Yes" : "No");
            cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
            cv::imshow(window_name, img_display);
            cv::waitKey(0);
        } else {
            cv::imwrite(output_image, img_display);
        }
    }

    return;
}
