Standby Blocker is a lightweight tool designed to prevent your system from entering standby mode but controllable via HTTP requests. I've wrote this to enable and disable automatic standby on my PC from Home Assistant automations.

As WinAPI is used, usage is limited to Windows.

# Usage
Standby Blocker can be run from a console window like follows:

```
standby-blocker.exe --port=8080 --token=f282ec57aeaa2ad77af71d71fa7e9c6e755b38f28a2b29582c8b01648bf5aeaa
```

The application can then be controlled with simple HTTP requests to http://localhost:8080/ to get the current status or http://localhost:8080/enable to disallow standby, and http://localhost:8080/disable to disable blocking.

If no token is supplied, no authorization is required. The port defaults to 8080.

To authenticate against the supplied token, set the Authorization Token in the request:

```
curl http://localhost:8080/enable -H "Authorization: Bearer f282ec57aeaa2ad77af71d71fa7e9c6e755b38f28a2b29582c8b01648bf5aeaa"
```

It is best be installed as a service, for example, with [nssm](https://nssm.cc/) or [srvany](https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/srvany). Don't forget to open the port in Windows firewall.