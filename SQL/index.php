<?php
$servername = "localhost";
$username = "root";
$password = "root";
$dbname = "skoleprojekt";

try {
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    // set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // prepare sql and bind parameters
    $stmt = $conn->prepare("INSERT INTO ejer (EjerNavn, TelefonNummer, EjerEmail) 
    VALUES (:EjerNavn, :TelefonNummer, :EjerEmail)");
    $stmt->bindParam(':EjerNavn', $navn);
    $stmt->bindParam(':TelefonNummer', $nummer);
    $stmt->bindParam(':EjerEmail', $email);

    // insert a row
    $navn = "Doe";
    $nummer = "10";
    $email = "john@example.com";
    $stmt->execute();

    echo $conn->lastInsertId();
    echo "New records created successfully";
    }
catch(PDOException $e)
    {
    echo "Error: " . $e->getMessage();
    }
$conn = null;
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>PDO SQL Test</title>
</head>
<body>
    
</body>
</html>