<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Document</title>

    <link rel="stylesheet" href="Opgaver/TicTacToe/tictactoe.css">
</head>
<body>
    <section id="menu">
        <h1>Let's play TicTacToe</h1>
        <label for="amount-of-fields" id="amount-of-fields-label">How many fields?</label>
        <input id="amount-of-fields" type="number" placeholder="Amount of fields">    
    </section>

    <table id="field"></table>
    
    <section id="victory">
        <p id="winner"></p>
    </section>
    <script src="Opgaver/TicTacToe/tictactoe.js"></script>
</body>
</html>