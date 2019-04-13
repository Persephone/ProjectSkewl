let animate = window.requestAnimationFrame ||
window.webkitRequestAnimationFrame ||
window.mozRequestAnimationFrame ||
function(callback) { window.setTimeout(callback, 1000/60) };

var isComputer = true;
var stop = false;
const startAIButton = document.getElementById('start-ai');
const start2PlayerButton = document.getElementById('start-2player');
const stopButton = document.getElementById('stop');

var canvas = document.getElementById('game-area');
const width = 400;
const height = 600;
canvas.width = width;
canvas.height = height;
var context = canvas.getContext('2d');

var player = null;
var player2 = null;
var computer = null;
var ball = null;

stopButton.addEventListener('click', () => {
    stop = true;
    stopButton.disabled = true;
    start2PlayerButton.disabled = false;
    startAIButton.disabled = false;
    context.clearRect(0, 0, canvas.width, canvas.height);
});

startAIButton.addEventListener('click', () => {
    stop = false;
    isComputer = true;
    stopButton.disabled = false;
    player = new Player(1);
    computer = new Computer();
    ball = new Ball(200, 300);
    startAIButton.disabled = true;
    start2PlayerButton.disabled = true;
    document.body.appendChild(canvas);
    animate(step);
});

start2PlayerButton.addEventListener('click', () => {
    stop = false;
    stopButton.disabled = false;
    isComputer = false;
    player = new Player(1);
    player2 = new Player(2);
    ball = new Ball(200, 300);
    startAIButton.disabled = true;
    start2PlayerButton.disabled = true;
    document.body.appendChild(canvas);
    animate(step);
});

var update = () => {
    player.update();
    if (isComputer) {
        computer.update(ball);
    } else {
        player2.update();
    }
    ball.update(player.paddle, isComputer ? computer.paddle : player2.paddle);
};

var step = () => {
    if (stop) {
        return;
    }

    update();
    render();
    animate(step);
};

function Paddle(x, y, width, height) {
    this.x = x;
    this.y = y;
    this.width = width;
    this.height = height;
    this.x_speed = 0;
    this.y_speed = 0;

    this.render = function() {
        context.fillStyle = "#0000FF";
        context.fillRect(this.x, this.y, this.width, this.height);
    }

    this.move = (x, y) => {
        this.x += x;
        this.y += y;
        this.x_speed = x;
        this.y_speed = y;

        if (this.x < 0) { // check left side
            this.x = 0;
            this.x_speed = 0;
        } else if (this.x + this.width > canvas.width) { // check right side
            this.x = 400 - this.width;
            this.x_speed = 0;
        }
    }
}

function Player(id) {
    this.id = id;
    this.paddle = id == 1 ? new Paddle(175, 590, 50, 10) : new Paddle(175, 0, 50, 10);
    this.render = () => this.paddle.render();
    this.update = () => {
        for (let key in keysDown) {
            const value = Number(key);
            if (this.id == 1) {
                if (value == 37) { // left arrow
                    this.paddle.move(-4, 0);
                } else if (value == 39) { // right arrow
                    this.paddle.move(4, 0);
                } else { // no movement
                    this.paddle.move(0, 0);
                }
            } else {
                if (value == 65) { // left arrow
                    this.paddle.move(-4, 0);
                } else if (value == 68) { // right arrow
                    this.paddle.move(4, 0);
                } else { // no movement
                    this.paddle.move(0, 0);
                }
            }
        }
    }
 }
 
function Computer() {
   this.paddle = new Paddle(175, 0, 50, 10);
   this.render = () => this.paddle.render();
   this.update = (ball) => {
       const x_pos = ball.x;
       let diff = -((this.paddle.x + (this.paddle.width / 2)) - x_pos);
       if (diff < 0 && diff < -4) { // max speed left
           diff = -5;
       } else if (diff > 0 && diff > 4) { // max speed right
           diff = 5;
       }

       this.paddle.move(diff, 0);
       if (this.paddle.x < 0) {
           this.paddle.x = 0;
       } else if (this.paddle.x + this.paddle.width > 400) {
           this.paddle.x = 400 - this.paddle.width;
       }
   }
}

function Ball(x, y) {
    this.x = x;
    this.y = y;
    this.x_speed = 0;
    this.y_speed = 3;
    this.radius = 5;

    this.render = () => {
        context.beginPath();
        context.arc(this.x, this.y, this.radius, 2 * Math.PI, false);
        context.fillStyle = "#000000";
        context.fill();
    }

    this.update = (paddle1, paddle2) => {
        this.x += this.x_speed;
        this.y += this.y_speed;
        let top_x = this.x - 5;
        let top_y = this.y - 5;
        let bottom_x = this.x + 5;
        let bottom_y = this.y + 5;

        if(this.x - 5 < 0) { // hitting the left wall
            this.x = 5;
            this.x_speed = -this.x_speed;
        } else if(this.x + 5 > width) { // hitting the right wall
            this.x = 395;
            this.x_speed = -this.x_speed;
        }
        
        if(this.y < 0 || this.y > height) { // a point was scored
            this.x_speed = 0;
            this.y_speed = 3;
            this.x = width / 2;
            this.y = height / 2;
        }

        if(top_y > 300) {
            if(top_y < (paddle1.y + paddle1.height) && bottom_y > paddle1.y && top_x < (paddle1.x + paddle1.width) && bottom_x > paddle1.x) {
              // hit the player's paddle
              this.y_speed = -3;
              this.x_speed += (paddle1.x_speed / 2);
              this.y += this.y_speed;
            }
        } else {
            if(top_y < (paddle2.y + paddle2.height) && bottom_y > paddle2.y && top_x < (paddle2.x + paddle2.width) && bottom_x > paddle2.x) {
              // hit the computer's paddle
              this.y_speed = 3;
              this.x_speed += (paddle2.x_speed / 2);
              this.y += this.y_speed;
            }
        }
    }
}

var render = function() {
  context.fillStyle = "#FF00FF";
  context.fillRect(0, 0, width, height);
  player.render();
  if (isComputer) {
      computer.render();
  } else {
      player2.render();
  }
  ball.render();
};

/* Key Controls */

var keysDown = {};

window.addEventListener("keydown", function(event) {
  keysDown[event.keyCode] = true;
});

window.addEventListener("keyup", function(event) {
  delete keysDown[event.keyCode];
});