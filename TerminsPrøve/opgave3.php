<?php

$array = array(10, 2, 7, 15, 9);

function calculateSum(array $array) {

    $total = 0;
    foreach($array as $number) {
        $total += $number;
    }
    
    return $total;
}

$sum = calculateSum($array);

function findMinAndMax(array $array, &$min, &$max) {

    $min = array_search(min($array), $array);
    $max = array_search(max($array), $array);
}

$min = 0;
$max = 0;

findMinAndMax($array, $min, $max);

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Opgave 3 - Arrays</title>
</head>
<body>
    <div id="container">
        <h3><?php print_r($array); ?></h3>

        <section id="sum">
            <p>Summon af arrayet: <?php echo calculateSum($array); ?></p>
        </section>

        <section id="minmax">
            <p>Min index: <?php echo $min ?> Max Index: <?php echo $max ?>.</p>
        </section>
    </div>
</body>
</html>