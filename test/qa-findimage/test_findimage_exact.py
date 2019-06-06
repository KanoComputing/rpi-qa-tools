#!/usr/bin/python
#
# Python pytest module for the qa-findimage, using exact images
#

import os
import json


def find_dashboard_icon(image_file):
    '''
    Runs the findimage tool, returns True if image_file is found
    '''
    qa_findimage='../tools/qa-findimage/qa-findimage'
    source_image='qa-findimage/data/kano-dashboard.png'
    data = os.popen('{} {} {}'.format(qa_findimage, source_image, image_file)).read()
    json_data = json.loads(data)
    return json_data['found'] == True
    

def test_find_minecraft():
    assert(find_dashboard_icon('qa-findimage/data/dashboard-minecraft.png') == True)

def test_find_scratch():
    assert(find_dashboard_icon('qa-findimage/data/dashboard-scratch.png') == True)

def test_find_projects():
    assert(find_dashboard_icon('qa-findimage/data/dashboard-projects.png') == True)
