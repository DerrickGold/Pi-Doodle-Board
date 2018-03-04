# Pi-Doodle-Board

First doodle your picture:
![PiDoodleBoard](/ScreenShots/doodlepi1.png?raw=true "Doodle On Pi")

Then show off your art:
![PiDoodleBoard](/ScreenShots/doodlepi2.jpg?raw=true "Doodle On Pi")

And tweet it afterwards!


## Setup Raspberry Pi

### Set up environmental variables for Twitter API

export TWITTER_API_KEY "YOUR API KEY"
export TWITTER_API_SECRET "YOUR API SECRET"
export TWITTER_TOKEN "YOUR TWITTER TOKEN"
export TWITTER_TOKEN_SECRET "YOUR TWITTER TOKEN SECRET"

### Compile the doodle application
    git clone https://github.com/DerrickGold/Pi-Doodle-Board.git
    cd DrawingPi/libBAG && make
    cd ../ && make

### Start the web server and doodle program
    python3 ../server.py &
    ./DrawingPi.bin


## Setup Remote Display

Open a web browser and go to http://YOUR_PI_IP_ADDRESS/index.html

