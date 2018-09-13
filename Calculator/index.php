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
    <link rel="stylesheet" href="/style.css">
    <title>Document</title>
</head>
<body>
    <form method="post" action="#">
        <section>
            <label for="number1">Tal 1:</label>
            <input type="number" id="number1"  name="number1" value="<?php echo isset($_POST['number1']) ? $_POST['number1'] : '' ?>"/>
        </section>
        
        <section>
            <label for="operator">Operator:</label>
            <select name="operator" width="161" style="width: 161px">
                <option value="+">+</option>
                <option value="-">-</option>
                <option value="*">*</option>
                <option value="/">/</option>
            </select>
        </section>

        <section>
            <label for="number2">Tal 2:</label>
            <input type="number" id="number2"  name="number2" value="<?php echo isset($_POST['number2']) ? $_POST['number2'] : '' ?>"/>
        </section>

        <section>
            <label for="result">Resultat:</label>
            <input type="number" id="result"   name="result" value="<?= $result ?>" readonly/>
        </section>

        <input type="submit" value="Beregn" class="button">
    </form>
</body>
</html>