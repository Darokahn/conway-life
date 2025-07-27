# Conway's Life

This is my Conway's Life implementation. It models each cell as a hardware data latch, and follows a simpler-than-typical ruleset.
Each live cell first increments the neighbor buffer of each of its neighbors.
Then, each uses two rules to determine its behavior: if live neighbor count == 3, set the data-in line to HIGH; else LOW. if neighbor count != 2, trigger latch.
It uses a null border in the screen buffer that prevents the necessity for bounds checking.

Very Linux-dependent.
