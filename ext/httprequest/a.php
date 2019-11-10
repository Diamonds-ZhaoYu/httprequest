<?php
$a = HttpRequest::current();
var_dump(get_class_methods('HttpRequest'));
var_dump($a->set("name","abcccccc")->set("age",10)->name);
var_dump($a->get("age"));
var_dump($a);

