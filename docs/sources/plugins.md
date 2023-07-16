libsir plugins      {#plugins}
=============

Using plugins, it is possible to extend libsir's reach when dispatching log messages. A simple example of this would be a plugin that only registered for emergency-level messages. Whenever it receives a message, it posts the message to a REST API endpoint which results in a push notification being sent to your mobile device.

Another example would be as part of some critical process, the application utilizing libsir is responsible for monitoring the state of said process, and using physical hardware (lights, audible tones), alert the humans present to a potential problem. This plugin would, upon receiving a critical-level or higher message, turn on a flashing red light and play an alert tone on an external speaker.

Anatomy of a libsir plugin
--------------------------

libsir's plugin interface will be versioned&mdash;with this initial release, we'll be starting at 1. Every time a new function is added (or one is modified), the version number will be bumped.

When plugins are compiled, they hard-code their interface version. This means that as libsir continues to evolve (and the version number increases), it can still communicate with older plugins via their versioned interface.


