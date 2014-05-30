kitty-arduino
=============

Code to run on kitty's arduino.

Depends on [CMPS10](https://github.com/kragniz/CMPS10) to interact with the
compass. Install it in the `lib` directory:

```bash
$ git clone https://github.com/kragniz/CMPS10.git lib
```

Now, if ino tool is set up correctly (if you're using
[kitty-provisioner](https://github.com/abersailbot/kitty-provisioner) this is
done for you), you can compile with:

```bash
$ ino build
$ ino upload
```
