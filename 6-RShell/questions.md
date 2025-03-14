1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_The remote client can continue receiving streams of data from the server until it encounters data that have been null-terminated. We can use a while loop to continue receiving this data from the remote client side and print to screen the data we receive. Once we receive data that has been null-terminated, we can exit the while loop because we rely on the server to send null-terminated data._

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_A networked shell protocol may be able to provide a header that specifies the length of each message so that it can detect the beginning and end of the command. If the networked shell protocol does not properly define and detect the beginning and end of a cmomand sent over by a TCP connection, then the receiver might misinterpret the data, reading less or more commands than expected._

3. Describe the general differences between stateful and stateless protocols.

_Stateful protocols will have more robust capabilities, but will generally be slower with more overhead. Stateless protocols will be faster, have less overhead, and will not be fully reliable._

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_UDP is "unreliable" in the sense that messages can get lost and we wouldn't know. However, it is significantly faster and more efficient than TCP. UDP is often used for video or audio streaming. Even if messages were lost during the process, we wouldn't necessarily be able to detect it in the video or audio streaming. In return, we get better performance in our video/audio streaming._

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_Sockets are the programming interface provided by the operating system enabling applications to write network programs._