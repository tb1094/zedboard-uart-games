# pong

Simple pong replica for the terminal; written in C. 

![Picture of the game while running.](good.png)

## How to run

Requires ncurses.

```bash
make ./pong 
```

## Customization

To edit the colors, see the `#defines` for `*_ATTRS` and look into the `setup`
to `init_pair`.

To edit the characters used for drawing the ball and pads, change `BALL` and
`PAD`.

To alter the size of the game field, adjust `WIDTH` and `HEIGHT`.

To change the pad size, edit `PAD_SIZE`.

To change the time between frames, modify `DELTA`.

To change the ball's velocity, set `velocity`. To change the ball's starting
angle, use `set_ball_angle(<angle>)` at the end of `setup`.

## License

[GNU GPLv3](LICENSE)
