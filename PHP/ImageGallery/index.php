<?php

require_once 'DirectoryReader.php';

$images = DirectoryReader('images');

if (!$images) {
    die('Could not load images');
}

?>

<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta http-equiv="X-UA-Compatible" content="ie=edge">
        <title>Gallery</title>

        <style>
            * {
                box-sizing: border-box;
            }

            body {
                background-color: lightblue;
                text-align: center;
                display: block;
            }

            .images {
                padding: 5px;
            }

            /* Clearfix (clear floats) */
            .images::after {
                content: "";
                clear: both;
                display: table;
            }

            img {
                height: 350px;
                width: 400px;
            }

            .column {
                float: right;
                width: 50%;
            }
        </style>
    </head>
    <body>
        <h1>Found <?php echo count($images); ?> images.</h1>
    
        <div class="images">
            <?php foreach ($images as $image): ?>
                <div class="column">
                    <img src="<?php echo $image; ?>">
                </div>
            <?php endforeach; ?>
        </div>

    </body>
</html>