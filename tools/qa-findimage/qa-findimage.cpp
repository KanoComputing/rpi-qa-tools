/**
 * findimage.cpp
 *
 * Copyright (C) 2019 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * A simple console tool to search an image contained in another image using
 * OpenCV matchTemplate algorithm.
 *
 * Read the official explanation to understand how this works:
 *     http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html
 *
 * Usage: findimage: <source image> <image to find> [output image | "ui"]
 *
 * A JSON string will be returned explaining if and where the image was found,
 * for example:
 *
 *     {
 *         "found": true,
 *         "x": 765,
 *         "y": 30,
 *         "width": 209,
 *         "height": 80,
 *         "minVal": -0.595378,
 *         "maxVal": 0.578674
 *     }
 *
 * If output image is provided, an image will be rendered with a green box
 * surrounding the match, or a red box marking the best approximation, if the
 * image couldn't be found (see accuracy_ratio variable).
 *
 * If output image is "ui" it will displayed on a window (non RPI systems at
 * this time).
 *
 * If source image is "dispmanx", the image to find will be searched for on the
 * current RPi screen.
 *
 * Should accept most major formats (png, jpg, ..)
 *
 */


#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>

#include <iostream>
#include <string>

#include "qa-findimage/rpi-screenshot.h"

using cv::Mat;
using cv::Point;

// function prototypes
void MatchingMethod(int, void*, const std::string &output_image);

// Global Variables
Mat img;
Mat templ;
Mat result;
Mat result_threshold;
double accuracy_ratio = 0.6f;


// TODO: command line
bool verbose = false;
bool ui = false;

// Provide this literal as the source image to find image on the RPi screen
#define CMDLINE_DISPMANX_SOURCE   "dispmanx"
#define CMDLINE_SHOW_UI           "ui"


int main(int argc, char** argv)
{
    std::string source_image;
    std::string image_to_find;

    if (argc < 3) {
        std::cout <<
            "Syntax: findimage <source image> <image to find> [output image]\n";
        return 1;
    } else {
        source_image = argv[1];
        image_to_find = argv[2];
    }

    // output image to graphically explain the match
    std::string output_image;
    if (argc == 4) {
        output_image = argv[3];
        if (output_image == CMDLINE_SHOW_UI) {
            // show it on the screen directly in a X11 window
            ui = true;
        }
    }

    // the magic name "rpi" for source image means a RaspberryPI screenshot
    if (!strcmp(CMDLINE_DISPMANX_SOURCE, source_image.c_str())) {
        int rows = 0;
        int cols = 0;
        void *rpi_screen = get_rpi_screenshot(verbose, &cols, &rows);

        if (!rpi_screen) {
            std::cout << "Error: could not take a dispmanx screenshot\n";
            return 1;
        } else {
            // CV_8UC3 means 8-bit unsigned integer matrix, with 3 bytes per
            // channel.
            img = Mat(rows, cols, CV_8UC3, rpi_screen);
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


void MatchingMethod(__attribute__((unused)) int count,
                    __attribute__((unused)) void *userdata,
                    const std::string &output_image)
{
    // Choose an alternative mathing method here
    // CV_TM_SQDIFF, CV_TM_SQDIFF_NORMED, CV_TM_CCORR
    // CV_TM_CCORR_NORMED, CV_TM_CCOEFF, CV_TM_CCOEFF_NORMED

    int match_method = CV_TM_CCOEFF_NORMED;

    // Source image to display
    Mat img_display;
    img.copyTo(img_display);

    // Create the result matrix
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    // The actual image search happens here
    result.create(result_rows, result_cols, CV_32FC1);
    cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, Mat());
    cv::matchTemplate(img, templ, result, match_method);

    // Localizing the best match with minMaxLoc
    double minVal;
    double maxVal;
    double accuracy_val;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;

    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all
    // the other methods, the higher the better
    if (match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED) {
        matchLoc = minLoc;
        accuracy_val = maxVal;
    } else {
        matchLoc = maxLoc;
        accuracy_val = maxVal;
    }

    // return a json to the console with the finding
    bool found = accuracy_val < accuracy_ratio ? false : true;
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

        // Either save an image to show finding, or open up a window (on non RPI
        // systems)
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
