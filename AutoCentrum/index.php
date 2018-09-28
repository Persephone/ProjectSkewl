<?php
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>AutoCentrum A/S</title>

    <link rel="stylesheet" href="styles/main.css">
    <link href="images/favicon.ico" rel="icon" />

    <script type="text/javascript" src="http://code.jquery.com/jquery-1.9.0.min.js"></script>
</head>
<body onload="refresh('info.php')"> 
    
<!-- Header Section -->
<div class="header">
    <?php include('includes/header.php') ?>
</div>

<!-- Navigation Section -->
<div class="nav">
    <ul>
        <?php include('includes/nav.php') ?>
    </ul>
</div>

<!-- JavaScript -->
<script type="text/javascript">
    function refresh($link)
    {
        $currentPage = $link;
        $('#content').load($link).fadeIn('slow');
    }
</script>

<!-- Main Section -->
<div class="row">
    <div class="side">
        <h2>About Me</h2>
        <h5>Photo of me:</h5>
        <div class="fakeimg" style="height:200px;">Image</div>
        <p>Some text about me in culpa qui officia deserunt mollit anim..</p>

    </div><!-- Side -->

    <div class="main">
        <div id="content"></div> <!-- Will be replaced with whatever navigation is selected -->
    </div> <!-- Main -->
</div><!-- Row -->

<div class="footer">
    <?php include('includes/footer.php') ?>
</div>

</body>
</html>