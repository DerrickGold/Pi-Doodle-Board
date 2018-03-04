#!/usr/bin/env python3
import twitter
import os
import sys
import subprocess
import logging

DEBUG=False

API_KEY = os.environ['TWITTER_API_KEY']
API_SECRET = os.environ['TWITTER_API_SECRET']
API_TOK = os.environ['TWITTER_TOKEN']
API_TOK_SECRET = os.environ['TWITTER_TOKEN_SECRET']
POST_TEXT="New doodle from a #RaspberryPi."



logging.basicConfig(filename='tweet.log', level=logging.DEBUG)

if API_KEY is None or API_SECRET is None or API_TOK is None or API_TOK_SECRET is None:
    logging.debug('Twitter credential environmental variables not set')
    exit(1)

#make sure an argument is provided
if len(sys.argv) < 2:
    logging.debug('Missing arguments: {}'.format(sys.argv))
    exit(1)

IMAGE_NAME=sys.argv[1]
logging.debug('Image path to tweet: {}'.format(IMAGE_NAME))

api = twitter.Api(consumer_key=API_KEY, consumer_secret=API_SECRET, \
                  access_token_key=API_TOK, access_token_secret=API_TOK_SECRET)

status = api.VerifyCredentials()
#user credentials were incorrect
if status is None:
    logging.debug('Failed to verify twitter credentials')
    exit(1)
    
#convert drawing to png for posting
OUTPUT_IMG = IMAGE_NAME + ".png"
TAGS = sys.argv[2]
logging.debug("TAGS: " + TAGS)

code = subprocess.call(["convert", IMAGE_NAME, OUTPUT_IMG])
if code == 0:
    if len(TAGS) > 0:
        if '#' not in TAGS:
            TAGS = "#" + TAGS
            
        POST_TEXT = POST_TEXT + "\n" + TAGS
            
    logging.debug("Post text: " + POST_TEXT)
    if DEBUG == False:
        api.PostUpdate(status=POST_TEXT, media=OUTPUT_IMG)
        
    logging.debug('Successfully tweeted image!')
    os.remove(OUTPUT_IMG)
else:
    logging.debug("Convert failed to convert {} to png".format(IMAGE_NAME))

