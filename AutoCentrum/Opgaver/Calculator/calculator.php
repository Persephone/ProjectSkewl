<?php
//var_dump($_POST);

function calculate($a, $b, $operator) {

    switch($operator)
    {
        case "+":
            return $a + $b;
        case "-":
            return $a - $b;
        case "*":
            return $a * $b;
        case "/":
            return $a / $b;
    }

    return 0;
}
$result = 0;

if (!empty($_POST))
    $result = calculate($_POST["number1"], $_POST["number2"], $_POST["operator"]);

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <link rel="stylesheet" href="Opgaver\Calculator\calculator.css">
    <title>Document</title>
</head>
<body>
    <div class="container">
    <form method="post" action="#">
        <label for="number1">Tal 1:</label>
        <input type="number" id="number1"  name="number1" value="<?php echo isset($_POST['number1']) ? $_POST['number1'] : '' ?>"/>
    
        <label for="operator">Operator:</label>
        <select name="operator">
            <option value="+">+</option>
            <option value="-">-</option>
            <option value="*">*</option>
            <option value="/">/</option>
        </select>

        <label for="number2">Tal 2:</label>
        <input type="number" id="number2"  name="number2" value="<?php echo isset($_POST['number2']) ? $_POST['number2'] : '' ?>"/>

        <label for="result">Resultat:</label>
        <input type="number" id="result"   name="result" value="<?= $result ?>" readonly/>

        <input type="submit" value="Beregn" class="button">
    </form>
</div>
</body>
</html>