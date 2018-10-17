<?php
    include_once 'header.php';
?>

<section class="main-container">
    <div class="main-wrapper">
        <div class="left-wrapper">
            <p>Test</p>
            <div class="image"></div>
        </div>
        <div class="right-wrapper">
            <?php
                if (isset($_SESSION['userid'])) {
                    echo "<p>You are logged in</p>";
                } else {
                    echo "<p>You are not logged in</p>";
                }
            ?>
        </div>
            
    </div>
</section>

<?php
    include_once 'footer.php';
?>
