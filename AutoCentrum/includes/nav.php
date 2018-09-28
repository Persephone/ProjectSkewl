<link rel="stylesheet" href="styles/nav.css">

<?php

    // Nav bar items
    $navItems = array(

        array(
            "link"    => "info.php",
            "title"   => "Home"
        ),
        array(
            "link"    => "news.php",
            "title"   => "News"
        ),
        array(
            "link"    => "contact.php",
            "title"   => "Contact"
        ),
        array(
            "link"    => "gallery.php",
            "title"   => "Galleri"
        ),
        array(
            "link"    => "Opgaver/TicTacToe/TicTacToe.php",
            "title"   => "Tic Tac Toe"
        ),
        array(
            "link"    => "Opgaver/FindHolger/FindHolger.php",
            "title"   => "Find Holger"
        ),
        array(
            "link"    => "Opgaver/Renter/renter.php",
            "title"   => "Renter"
        ),
        array(
            "link"    => "Opgaver/Calculator/calculator.php",
            "title"   => "Lommeregner"
        ),
    );

    foreach ($navItems as $item) {
        echo '<li><a onclick="refresh(\''.$item['link'].'\');">'.$item['title'].'</a></li>';
    }
?>