# Final-Assignment-network-programming

Network Programming for Games 1 - 5SD808 54808

Name: Sebastian Saastamoinen

Email: sebastian.saastamoinen.3769@student.uu.se or sebastian.jv.saastamoinen@outlook.com

Description: This is an assignment where the goal is to implement multiplayer features for a classic video game using udp.
             Additionally there were features that had to be implemented to make the multiplayer experience better like remote entity interpolation,
             client side prediction and reconciliation. The multiplayer aspect was the primary goal for this assignment with the classic game being a vehicle to
             express that the implementation worked. 

Controls - 
    W - Move up
    S - Move down
    A - Move left
    D - Move right
    V - Place bomb
    L - Start game from lobby screen immediately(Press it in the screen before the game starts and it will start regardless of amount of players, if there is only one player present however they will immediately win)
    M - Force a client to stop transmitting packages(Made to test the time out of clients)

Bombs explode 6 seconds after placement. Bombs kill both the player who placed them and the other players. The player is reconciled when they are more than 1 tiles from the server position. 
The game begins rougly 15 - 30 sec after 3 players have joined. When every player except one dies the game transitions to a post game and disconnect all the players. 

I made this project with Theodor Rydberg, we decided to implement the game ourselves when our implementations differed too greatly but we still shared code and some of the code is Theos and i used ai to mainly check errors.