<?php
$a = HttpRequest::current();
echo "<pre>";
print_r(get_class_methods('HttpRequest'));
//var_dump($a->set("name","abcccccc")->set("age",10)->name);

var_dump($a);
var_dump($_SERVER);
var_dump($a->isPost());
$name = HttpRequest::current()->name;
var_dump($name);

HttpRequest::current()->age;


