<?php
$dir = '.';
$files = scandir($dir);
print_r($files);

$result = "";
foreach($files as $f) {
    if (strpos($f, '.'))
        continue;
    
    $result .= "<li><a href='/Opgaver/$f'>$f</a></li>";
}
?>

<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Document</title>
</head>

<body>
<ul>
    <?= $result ?>
</ul>

</body>

</html>
