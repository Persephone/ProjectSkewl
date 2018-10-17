<?php
    include_once 'header.php';
    require_once 'includes/dbh.inc.php'
?>

<link rel="stylesheet" type="text/css" href="Styles/sign-up.css">

<section class="main-container">
    <div class="main-wrapper">
        <h2>Sign Up</h2>

        <form class="signup-form" action="includes/sign-up.inc.php" method="POST">
            <input type="text" name="firstname" placeholder="Firstname">
            <input type="text" name="lastname"  placeholder="Lastname">
            <input type="text" name="email"     placeholder="E-mail">
            <input type="text" name="username"  placeholder="Username">
            <input type="password" name="password"  placeholder="Password">
            <button type="submit" name="submit">Sign up</button>
        </form>
    </div>
</section>

<?php
    include_once 'footer.php';
?>
