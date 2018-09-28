<?php

$result = 0;

if ($_SERVER["REQUEST_METHOD"] == "POST") {

    $number1 = $_POST['number1'];
    $number2 = $_POST['number2'];
    $number3 = $_POST['number3'];

    $array = array($number1, $number2, $number3);

    $result = max($array);
}

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Document</title>

    <link rel="stylesheet" href="styles/main.css">
</head>
<body>
    <div class="container">
        <form action="#" method="post">
            <div id="numbers">
                <input type="text" name="number1" id="number1" value="<?php echo isset($_POST['number1']) ? $_POST['number1'] : '' ?>"/>
                <input type="text" name="number2" id="number2" value="<?php echo isset($_POST['number2']) ? $_POST['number2'] : '' ?>"/>
                <input type="text" name="number3" id="number3" value="<?php echo isset($_POST['number3']) ? $_POST['number3'] : '' ?>"/>
            </div>

        <input type="submit" value="Submit">
        </form>

        <p>Højeste tal:</p>
        <input type="text" id="result" value="<?php if (isset($result)) echo $result ?>"" readonly>
    </div>
</body>
</html>