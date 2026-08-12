#include "codexion.h"


- We have to have a monitor function that will keep on running until a coder burns out or 
they are get to the number of compiles required 

Ideas

- A queue of ids and timestamps of when that coder will burn out 
- This queue is ordered from the coder with the most recent burn time to the furthest 
- It is easy to keep track of this queue, by the time each coder compiles we remove its
spot on the queue and place him the furthest on the list 
- This queue is shared by all theads so we need to have a mutex for that will be shared by all of them and that mutex will allow us to change the queue 
- When queue changes a signal is sent and we would then just need to know if we stopped 
because a coder burned or if there was another compilation done 
