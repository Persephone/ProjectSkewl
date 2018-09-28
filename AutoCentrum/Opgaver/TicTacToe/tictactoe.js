/**
 * IF someone has filled out the board horizontally, vertically, or across
 * @param {array} playingField 
 * @returns {string} winner - Returns the 
 */
function checkIfWinner(field) {
    // Check rows
    for (let row = 0; row < field.length; row++) {
        
        // This flag will be set to false if the rows aren't identical
        let rowFlag = true;
        field[row].forEach(e => {
            // Will set the flag to false if one of the fields in each row isn't irentical to the first element in the row
            if (field[row][0] !== e)
                rowFlag = false;
        });

        // If all elements in are identical then return that row
        if (rowFlag)
            return field[row][0];

    }

    // Check columns
    // Start off by
    for (let column = 0; column < field[0].length; column++) {
        let prev = field[0][column]; 
        let columnFlag = true;

        // Check all the rows in the current column two at a time
        // If one doesn't match the flag will be set to false and there is no winner
        field.forEach(row => {
            if (row[column] !== prev)
                columnFlag = false;

            prev = row[column];
        });

        if (columnFlag)
            return prev;
    }

    // Check across using basically the same principal as with the column checks
    let i = 0;
    let j = field.length - 1;
    
    let prevI = field[i][i];
    let prevJ = field[i][j];

    let iFlag = true;
    let jFlag = true;

    while (i < field.length && j >= 0) {
        field[i][i];
        field[i][j];

        if (field[i][i] !== prevI)
            iFlag = false;

        if (field[i][j] !== prevJ)
            jFlag = false;

        prevI = field[i][i];
        prevJ = field[i][j];
                
        i++;
        j--;
    }

    if (iFlag) {
        return field[0][0];
    }

    if (jFlag) {
        return field[0][field.length - 1];
    }

    // No winner so far
    return null;
}

// The playing field as the program sees it
let board = new Array();

/**
 * Generates a new n*n array filled up with null and returns it
 * @param {number} size - The height and width of the playingfield
 * @returns {array} board - The new n*n board
 */
function generateBoard(size) {
    const newBoard = new Array(size);
    for (let i = 0; i < size; i++) {
        newBoard[i] = new Array(size);
        for (let j = 0; j < size; j++) 
            newBoard[i][j] = null;
    }
    return newBoard;
}
/**
 * When the player clicks the handle what happens when the player clicks, and if the player hasn't won, then handle the enemies click
 * @param {*} event 
 */
function playerClick(event) {
    const currentField = event.target;
    // If clicked on own or enemy square
    if (currentField.innerText !== "") {
        currentField.style.backgroundColor = "#ff0000";
        return;
    }

    currentField.innerText = "x";
    
    const row    = parseInt(currentField.getAttribute("data-row"));
    const column = parseInt(currentField.getAttribute("data-column"));

    board[row][column] = "x";

    const winnerCheck = checkIfWinner(board);
    if (!winnerCheck) {
        enemyClick();
    } else {
        presentWinner("Player");
    }
} 

/**
 * When the enemy clicks, get all remaining fields and place the enemies
 */
function enemyClick() {
    const fields = document.querySelectorAll(".square");
    const available = [];

    fields.forEach(field => {
        if (field.innerText == "") {
            available.push(field);
        }
    });

    // There are no fields left to fill out
    if (available.length === 0) {
        presentWinner("Stalemate")
        return;
    }

    const selectedField = available[Math.floor(Math.random() * available.length)];
    const row           = parseInt(selectedField.getAttribute("data-row"));
    const column        = parseInt(selectedField.getAttribute("data-column"));
    
    selectedField.innerText = "o";

    board[row][column] = "o";

    const winnerCheck = checkIfWinner(board);

    if (winnerCheck)
        presentWinner("AI");

    // The enemy checked the last field without winning
    if (available.length === 1)
        presentWinner("Stalemate");
}

/**
 * 
 * @param {string} winnerName - Name of the winner
 */
function presentWinner(winnerName) {
    const winningMessage = winnerName === "Player"   ? "Congratulations, you won!" :
                           winnerName === "AI"       ? "Aww, you lost" :
                                                        "Stalemate!"

    // Grey out fields
    const fields = document.querySelectorAll(".square");
    fields.forEach(field => {
        field.style.pointerEvents = "none";
        //field.removeEventListener("click", playerClick);
        field.style.backgroundColor = "#cccccc";
    });

    const winnerTextContainer = document.getElementById("winner");
    winnerTextContainer.innerText = winningMessage;
}

function setup(event) {
    if (event.keyCode !== 13) // enter button
        return;

                  document.getElementById("winner").innerText = "";
    const label = document.getElementById("amount-of-fields-label");
    
    if (!event.target.value) {
        event.target.placeholder = "This cannot be empty";
        event.target.style.borderColor = "#ff0000";
        return;
    }


    const playingField  = document.getElementById("field");

    const square = parseInt(event.target.value);

    if (isNaN(square)) {
        label.style.backgroundColor = "#FF0000";
        label.innerText = "You need to enter a number!";
        return;
    } else {
        label.style.backgroundColor = "#FFFFFF";
        label.innerText = "How many fields?";
    }

    playingField.innerText = "";

    for (let i = 0; i < square; i++) {
        const tr = document.createElement("tr");

        for (let j = 0; j < square; j++) {
            const td = document.createElement("td");
            td.classList += "square";
            td.setAttribute("data-row", i);
            td.setAttribute("data-column", j);

            td.addEventListener("click", playerClick);

            tr.appendChild(td);
        }

        playingField.appendChild(tr);
    }

    board = generateBoard(square);
}


const questionField = document.getElementById("amount-of-fields");
questionField.addEventListener("keydown", setup);