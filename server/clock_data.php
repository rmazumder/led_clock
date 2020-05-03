<?php

$first = floor(date("h")/10);
$second = date("h")%10;
$third = floor(date("i")/10);
$fourth = date("i")%10;
$R = 0;
$G = 255;
$B = 0;
$brightness = 255;
$buzzer = 0;
$data = $first.':'.$second.':'.$third.':'.$fourth.':'.$R.':'.$G.':'.$B.':'.$brightness.':'.$buzzer;
echo $data;


?>
