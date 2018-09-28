<!DOCTYPE html>
<html lang="en">

<head>
    <link rel="stylesheet" href="styles/contact.css">
</head>

<body>

<h3>Kontakt Form</h3>

<div class="container">
  <form action="contact_process.php" method="post">
    <label for="fname">Navn</label>
    <input type="text" id="fname" name="firstname" placeholder="Navn..." autofocus>
    <span class="error"><?php $name_error ?></span>

    <label for="lname">Efternavn</label>
    <input type="text" id="lname" name="lastname" placeholder="Efternavn...">
    <span class="error"><?php $lastname_error ?></span>

    <label for="age">Alder</label>
    <input type="number" id="fage" name="age" placeholder="Alder...">
    <span class="error"><?php $age_error ?></span>

    <label for="tel">Telefonnummer</label>
    <input type="tel" id="ftel" name="tel" placeholder="22446699">
    <span class="error"><?php $phone_error ?></span>

    <label for="email">E-Mail</label>
    <input type="email" id="femail" name="email" placeholder="mail@mail.com">
    <span class="error"><?php $email_error ?></span>

    <label for="subject">Besked</label>
    <textarea id="subject" name="subject" placeholder="Tekst..." style="height:200px"></textarea>
    <span class="error"><?php $message_error ?></span>
    
    <input type="submit" value="Send" data-submit="...Sending">
  </form>
</div>
</body>
</html>