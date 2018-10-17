<?php

session_start();

if (isset($_POST['submit'])) {

    include 'dbh.inc.php';

    $username = mysqli_real_escape_string($conn, $_POST['username']);
    $password = mysqli_real_escape_string($conn, $_POST['password']);

    # Error handlers
    if (empty($username) || empty($password)) {
        header("Location: ../index.php?login=empty");
        exit();
    }

    # Check if user exists
    $sql = "SELECT * FROM users WHERE Username = '$username' OR Email='$username'";
    $result = mysqli_query($conn, $sql);
    $resultCheck = mysqli_num_rows($result);

    # User not found
    if ($resultCheck < 1) {
        header("Location: ../index.php?login=error");
        exit();
    }

    # Check if his password matches
    if ($row = mysqli_fetch_assoc($result)) {
        # De-Hashing the password
        $hashedPwdCheck = password_verify($password, $row['Password_Hash']);
        if ($hashedPwdCheck == true) {
            # All Good - Log In User
            $_SESSION['userid']     = $row['Id'];
            $_SESSION['firstname']  = $row['FirstName'];
            $_SESSION['lastname']   = $row['LastName'];
            $_SESSION['email']      = $row['Email'];
            $_SESSION['username']   = $row['Username'];
            header("Location: ../index.php?login=success");
            exit();
        } elseif ($hashedPwdCheck == false) {
            # Passwords did not match
            header("Location: ../index.php?login=error");
            exit();
        }    
    }    

} else {
    header("Location: ../index.php?login=error");
    exit();
}