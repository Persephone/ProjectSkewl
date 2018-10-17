<?php

# Check if user submitted anything
if (isset($_POST['submit'])) {
    
    include_once 'dbh.inc.php';

    $firstname  = mysqli_real_escape_string($conn, $_POST['firstname']);
    $lastname   = mysqli_real_escape_string($conn, $_POST['lastname']);
    $email      = mysqli_real_escape_string($conn, $_POST['email']);
    $username   = mysqli_real_escape_string($conn, $_POST['username']);
    $password   = mysqli_real_escape_string($conn, $_POST['password']);

    # Error handling
    # Check for empty fields
    if (empty($firstname) || empty($lastname) || empty($email) || empty($username) || empty($password)) {
        header("Location: ../sign-up.php?sign-up=empty");
        exit();
    }

    # Check if input characters are valid
    if (!preg_match("/^[a-zA-Z]*$/", $firstname) || !preg_match("/^[a-zA-Z]*$/", $lastname)) {
        header("Location: ../sign-up.php?sign-up=invalid");
        exit();
    }

    # Check if email is valid
    if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
        header("Location: ../sign-up.php?sign-up=invalid-email");
        exit();
    }

    # Check if user already exists
    $sql = "SELECT * FROM users WHERE Username='$username';";
    $result = mysqli_query($conn, $sql);
    $resultCheck = mysqli_num_rows($result);
    
    if ($resultCheck > 0) {
        header("Location: ../sign-up.php?sign-up=user-taken");
        exit();
    }

    # Hash Password
    $hashedPwd = password_hash($password, PASSWORD_DEFAULT);
    # Insert the user
    $sql = "INSERT INTO users (`FirstName`, `LastName`, `Email`, `Username`, `Password_Hash`)
     VALUES ('$firstname', '$lastname', '$email', '$username', '$hashedPwd');";
    $query = mysqli_query($conn, $sql);
    if ($query) {
        header("Location: ../sign-up.php?sign-up=success");
    } else {
        echo mysql_errno($conn) . ": " . mysql_error($conn) . "\n";
    }
    exit();
} else {
    header("Location: ../sign-up.php");
    exit();
}
