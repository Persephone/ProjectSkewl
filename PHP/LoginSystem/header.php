<?php
    session_start();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Login System</title>
    <link rel="stylesheet" type="text/css" href="Styles/main.css">
</head>
<body>
    
<header>
    <nav>
        <div class="main-wrapper">
            <ul>
                <li><a href="index.php">Home</a></li>
                <li><a href="index.php">Test</a></li>
                <li><a href="index.php">Test</a></li>
            </ul>

            <div class="nav-login">
                <?php
                    if (isset($_SESSION['userid'])) {
                        echo
                        '<form action="includes/logout.inc.php" method="POST">
                                <button type="submit" name="submit">Logout</button>
                            </form>';
                    } else {
                        echo
                        '<form action="includes/login.inc.php" method="POST">
                            <input type="text" name="username" placeholder="Username/Email">
                            <input type="password" name="password" placeholder="Password">
                            <button type="submit" name="submit">Login</button>
                        </form>

                        <a href="sign-up.php">Sign Up</a>';
                    }
                ?>
            </div>
        </div>
    </nav>
</header>