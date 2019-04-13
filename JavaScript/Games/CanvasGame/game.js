// Cross-browser support for requestAnimationFrame
var requestAnimFrame = (function () {
    return window.requestAnimationFrame ||
        window.webkitRequestAnimationFrame ||
        window.mozRequestAnimationFrame ||
        window.oRequestAnimationFrame ||
        window.msRequestAnimationFrame ||
        function (callback) {
            window.setTimeout(callback, 1000 / 60);
        };
})();

var then = Date.now();
var gameArea;
var hero;
var monster;

class GameArea {
    constructor() {
        this.canvas = document.getElementById("canvas");
        this.ctx = canvas.getContext("2d");

        // Background
        this.Image = new Image();
    }

    Create() {
        this.canvas.width = 512;
        this.canvas.height = 480;
        this.Image.src = "images/background.png";
    }
}

class Hero {
    constructor() {
        this.Image = new Image();
        this.monstersCaught = 0;

        // Position
        this.x = 0;
        this.y = 0;

        // Speed
        this.speed = 256;
    }

    Create() {
        this.Image.src = "images/hero.png";
    }

    CaughtMonster() {
        ++this.monstersCaught;
    }
}

class Monster {
    constructor() {
        this.Image = new Image();

        // Position
        this.x = 0;
        this.y = 0;
    }

    Create() {
        this.Image.src = "images/monster.png";
    }
}

// Handle keyboard controls
var keysDown = {
};

addEventListener("keydown", function (e) {
    keysDown[e.keyCode] = true;
}, false);

addEventListener("keyup", function (e) {
    delete keysDown[e.keyCode];
}, false);

// Reset the game when the player catches a monster
var reset = function (first) {
    if (first)
    {
        hero.x = canvas.width / 2;
        hero.y = canvas.height / 2;
    }

    // Throw the monster somewhere on the screen randomly
    monster.x = 32 + (Math.random() * (canvas.width - 64));
    monster.y = 32 + (Math.random() * (canvas.height - 64));
};

// Update game objects
const update = function (modifier) {
    if (38 in keysDown) { // Player holding up
        hero.y -= hero.speed * modifier;
    }
    if (40 in keysDown) { // Player holding down
        hero.y += hero.speed * modifier;
    }
    if (37 in keysDown) { // Player holding left
        hero.x -= hero.speed * modifier;
    }
    if (39 in keysDown) { // Player holding right
        hero.x += hero.speed * modifier;
    }

    // Check for collision
    if (
        hero.x <= (monster.x + 32)
        && monster.x <= (hero.x + 32)
        && hero.y <= (monster.y + 32)
        && monster.y <= (hero.y + 32)
    ) {
        hero.CaughtMonster();
        reset(false);
    }
};

// Draw everything
const render = function () {
    // Refresh objects
    gameArea.ctx.drawImage(gameArea.Image, 0, 0);
    gameArea.ctx.drawImage(hero.Image, hero.x, hero.y);
    gameArea.ctx.drawImage(monster.Image, monster.x, monster.y);

    // Score
    gameArea.ctx.fillStyle = "rgb(250, 250, 250)";
    gameArea.ctx.font = "24px Helvetica";
    gameArea.ctx.textAlign = "left";
    gameArea.ctx.textBaseline = "top";
    gameArea.ctx.fillText("Goblins Caught: " + hero.monstersCaught, 32, 32);
};

// The main game loop
const main = function () {
    var now = Date.now();
    var delta = now - then;

    update(delta / 1000);
    render();

    then = now;

    // Request to do this again ASAP
    requestAnimationFrame(main);
};

function startGame() {
    console.log("Starting Game");

    gameArea = new GameArea();
    gameArea.Create();

    hero = new Hero();
    hero.Create();

    monster = new Monster();
    monster.Create();

    reset(true);
    main();
}

$('.button.play').click(function () {
    $('.main').hide();
    startGame();
});