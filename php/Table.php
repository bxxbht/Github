
<!DOCTYPE html>
<html>
<title>表单提交开始</title>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<body>
     <a href="../index.php">返回首页</a>
     <hr></hr>
<marquee fgcolor=Olive>同志们，看呀，我会走了耶！！</marquee>
<b>一、表单提交到另一个php上</b><br />
<form action="formTo.php" method="POST">
    Name:  <input type="text" name="username"><br />
    Email: <input type="text" name="email"><br />
    <input type="submit" name="submit" value="提交" />
</form>
<hr/>
<b>二、将一个表单 POST 给自己并在提交时显示数据：</b><br />
<br/>
<?php
if (isset($_POST['action']) && $_POST['action'] == 'submitted') {
    echo '<pre>';
 
    print_r($_POST);
    echo '<a href="'. $_SERVER['PHP_SELF'] .'">返回重填</a>';
 
    echo '</pre>';
} else {
?>
<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post">
    Name:  <input type="text" name="personal[name]"><br />
    Email: <input type="text" name="personal[email]"><br />
    Beer: <br>
    <select multiple name="beer[]">
        <option value="warthog">Warthog</option>
        <option value="guinness">Guinness</option>
        <option value="stuttgarter">Stuttgarter Schwabenbr</option>
    </select><br />
    <input type="hidden" name="action" value="submitted" />
    <input type="submit" name="submit" value="submit me!" />
</form>
<?php
}
?>
</body>
</html>
