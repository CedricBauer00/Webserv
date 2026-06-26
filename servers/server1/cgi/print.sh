#!/bin/sh

echo "Content-Type: text/plain\r"
echo "\r"

echo "Method: $REQUEST_METHOD\r"
echo "Script: $SCRIPT_NAME\r"
echo "Query:  $QUERY_STRING\r"
echo "Client: $REMOTE_ADDR\r"
echo "Host:   $SERVER_NAME\r"
echo "Port:   $SERVER_PORT\r"
echo "Proto:  $SERVER_PROTOCOL\r"
echo "CGI:    $GATEWAY_INTERFACE\r"

echo "\r"
echo "pwd:    $(pwd)\r"
input=$(cat)
echo "------POST BODY FOLLOWS-------"
echo "$input"