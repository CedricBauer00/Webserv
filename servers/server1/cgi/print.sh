#!/bin/sh

echo "Content-Type: text/plain"
echo

echo "Method: $REQUEST_METHOD"
echo "Script: $SCRIPT_NAME"
echo "Query:  $QUERY_STRING"
echo "Client: $REMOTE_ADDR"
echo "Host:   $SERVER_NAME"
echo "Port:   $SERVER_PORT"
echo "Proto:  $SERVER_PROTOCOL"
echo "CGI:    $GATEWAY_INTERFACE"