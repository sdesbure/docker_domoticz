Domoticz
======

Domoticz - https://github.com/zone117x/Jackett

Latest Domoticz git release.

**Pull image**

```
docker pull sdesbure/alpine_domoticz

```

**Run container**

```
docker run -d -p 8080:8080 --name=<container name> -v <path for config files>:/config -v /etc/localtime:/etc/localtime:ro sdesbure/alpine_domoticz
```

Please replace all user variables in the above command defined by <> with the correct values.

**Access Domoticz**

```
http://<host ip>:8080
```
