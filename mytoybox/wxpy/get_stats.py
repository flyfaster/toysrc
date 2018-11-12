#!/usr/bin/env python3
# Python program to get stats of video
import os
import sys
import hashlib
import argparse
import platform
import datetime
import time
import wx
import math
import heapq
import urllib.request
import urllib.parse
import re
"""
title="I like this" aria-label="like this video along with 1,127 other people" data-force-position="true" data-position="bottomright" data-orientation="vertical"><span class="yt-uix-button-content">1,127</span></button>
 title="I dislike this" aria-label="dislike this video along with 121 other people" data-force-position="true" data-position="bottomright" data-orientation="vertical"><span class="yt-uix-button-content">121</span></button>
"""


def get_video_stats(link):
    page = urllib.request.urlopen(link)
    page_content = page.read().decode()
    likes = re.search("aria-label=\"like this video along with (\d*.\d*.\d*)", page_content).group(1)
    dislikes = re.search("aria-label=\"dislike this video along with (\d*.\d*.\d*)", page_content).group(1)
    title = re.search("property=\"og:title\" content=\"([^\n]*)", page_content).group(1)
    return (likes, dislikes, title)

video_links = [
"https://www.youtube.com/watch?v=t9aDnTQPLAo"
]

if __name__ == "__main__":
    for link in video_links:
        stats = get_video_stats(link)
        print(stats[0] + " likes, " + stats[1] + " dislikes, title: " + stats[2])
