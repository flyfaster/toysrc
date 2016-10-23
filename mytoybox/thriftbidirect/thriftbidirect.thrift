service HandshakeService{
    oneway void HandShake();
}

service CallbackService{
    oneway void Push(1: string msg); 
}