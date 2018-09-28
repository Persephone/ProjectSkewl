<?php

$name_error = $lastname_error = $email_error = $phone_error = $age_error = $message_error = "";
$name = $email = $age = $lastname = $age = $message= "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {

    if (empty($_POST["name"])) {
        $name_error = "Navn er påkrævet";
    } else {
        $name = test_input($_POST["name"]);

        if (!preg_match("/^[a-zA-Z]*$/", $name)) {
            $name_error = "Kun bokstaver er tilladt.";
        }
    }

    if (empty($_POST["lastname"])) {
        $name_error = "Efternavn er påkrævet";
    } else {
        $name = test_input($_POST["lastname"]);

        if (!preg_match("/^[a-zA-Z]*$/", $lastname)) {
            $lastname_error = "Kun bokstaver er tilladt.";
        }
    }

    if (empty($_POST["email"])) {
        $name_error = "Email er påkrævet";
    } else {
        $name = test_input($_POST["email"]);

        if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
            $email_error = "Ugyldig email";
        }
    }

    if (empty($_POST["age"])) {
        $age_error = "Alder er påkrævet";
    } else {
        $age = test_input($_POST["email"]);

        if (!preg_match("/^0-9$/", $age)) {
            $age_error = "Ugyldig alder";
        }
    }

    if (empty($_POST['message'])) {
        $message = "";
    } else {
        $message = test_input($_POST['message']);
    }
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>AutoCentrum A/S</title>

    <link rel="stylesheet" href="styles/main.css">
    <link rel="stylesheet" href="styles/contact.css">
    <link href="images/favicon.ico" rel="icon" />

    <script type="text/javascript" src="http://code.jquery.com/jquery-1.9.0.min.js"></script>
</head>
<body> 
    
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
        <p>success!</p>
    </div> <!-- Main -->
</div><!-- Row -->

<div class="footer">
    <?php include('includes/footer.php') ?>
</div>

</body>
</html>