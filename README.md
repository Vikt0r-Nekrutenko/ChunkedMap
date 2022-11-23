# ChunkedMap
Simple demonstration of chunked map with preloaded cache
![view](https://scontent.flwo4-1.fna.fbcdn.net/v/t39.30808-6/315858384_167482635901605_7652724936059952543_n.jpg?_nc_cat=110&ccb=1-7&_nc_sid=730e14&_nc_ohc=YCjMsJ44DCoAX_OmCIQ&_nc_ht=scontent.flwo4-1.fna&oh=00_AfBRoqVbEv0uVY4H7UoCZUvvQeL5tDdsICY5uw75kdjuLA&oe=63801CB9)

**Now this project included in stf lib and next screen demonstrate abilities:**
* low use New Delete operations
* low use read from far memory(files) operations
* high life timing of chunks (10 seconds)
* high fps for full drawing the chunks one by one

AND THE MOST IMPORTANT
NEXT SCREEN DEMONTRATE MAP THAT HAS 3000x3000 CHUNKS!!!
```
stf::sdb::ChunkedMapT<Chunk>{3000, 3000};
```
where every chunk has 8x8 cells that present symbols. i guess it's awesome :)

![view2](https://scontent.fiev22-1.fna.fbcdn.net/v/t39.30808-6/316409345_167914455858423_3012120143325328283_n.jpg?_nc_cat=108&ccb=1-7&_nc_sid=730e14&_nc_ohc=XelQagqE2c0AX_-g0pu&_nc_ht=scontent.fiev22-1.fna&oh=00_AfDVt_FfXBjefKTE1zpQYEQZ2x6DdHzCfrAl1FgtHjLZxg&oe=63820CBB)

*Copyright (C) 2022 Viktor Nekrutenko*
