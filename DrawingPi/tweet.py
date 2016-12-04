#!/usr/bin/env python3
import twitter
import os
import sys
import subprocess

API_KEY = os.environ['TWITTER_API_KEY']
API_SECRET = os.environ['TWITTER_API_SECRET']
API_TOK = os.environ['TWITTER_TOKEN']
API_TOK_SECRET = os.environ['TWITTER_TOKEN_SECRET']
POST_TEXT="New doodle from a #RaspberryPi."

#make sure an argument is provided
if len(sys.argv) < 2: exit(1)

IMAGE_NAME=sys.argv[1]

api = twitter.Api(consumer_key=API_KEY, consumer_secret=API_SECRET, \
                  access_token_key=API_TOK, access_token_secret=API_TOK_SECRET)

status = api.VerifyCredentials()
#user credentials were incorrect
if status is None: exit(1)

#convert drawing to png for posting
OUTPUT_IMG = IMAGE_NAME + ".png"
subprocess.call(["convert", IMAGE_NAME, OUTPUT_IMG])

api.PostUpdate(status=POST_TEXT, media=OUTPUT_IMG)
os.remove(OUTPUT_IMG)


