# SFX 3 - Package Managment

One of the most important parts of a language is how it handles dependencies.

Skull tries to streamline the way your code interacts with packages by:
* Using Git as your packaging system
* Utilizing automatic package resolution
* Notifying you when updates are available

## Git as Your Package Manager

Most programming languages rely on third-party sites to host your packages.

With Skull, your packages will be released, downloaded, and updated through Git itself:

```
$ git add example-package/

$ git commit -m "added package for release 1.0"

$ git tag -a v1.0 -m "Release 1.0"

$ git push origin v1.0
```

In code:

```
from repo/name@v1.0 import example-package
```

Instead of `@v1.0` you can use:
* `@latest` for most recently tagged version
* `@breaking` for most recent Git commit
* `@<SHA-HERE>` for a specific Git SHA
* No `@` prefix: default to `@latest`

## Automatic package resolution

Skull will keep track of your installed packages, and warn you about conflicting packages when updating.

## Update Notifications

After a given time period, Skull will check for available packages in your project.

Skull will tell you if your package can be updated automatically, or if there were conflicts.