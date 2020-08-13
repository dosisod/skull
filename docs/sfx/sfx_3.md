# SFX 3 - Plugins

One of the most important parts of a language is how it handles dependencies.

Skull tries to streamline the way your code interacts with 3rd party code by:
* Using "Plugins", instead of "Packages"
* Using Git directly for managing plugins
* Always using the most up-to-date versions

## Using Plugins, not Packages

Instead of packages, which are prone to version conflicts and exponential depencencies, plugins are designed to be more light weight, and less relient on outside code.

Packages may include all of the versions of the sub-packages they use, but a plugin simply defines all of the other plugins that it uses. No versions, no conflicts!

You may be wondering, "wont this cause more version conflicts"? Keep reading, I will hopefully convince you otherwise.

## Using Git for Depencencies

Most programming languages rely on third-party sites to host your code.

With Skull, you instead make plugins that will be released, downloaded, and updated through Git itself:

```
$ git add example-plugin/

$ git commit -m "added code for release 1.0"

$ git tag -a v1.0 -m "Release 1.0"

$ git push origin v1.0
```

In Skull:

```python
from repo/name import example-plugin
```

This will pull in the most recently RELEASED version of the plugin.

Doing the following:

```python
# subject to change
from repo/name/bleeding import example-module
```

This will pull in the most recent COMMIT from that plugin (in Git that is).

## Continuously Updated Plugins

Plugins are versioned, like packages. But, unlike packages, plugins in Skull will always be kept at the most recent version.

Why? For a few reasons:
* Relying on outdated package versions will discourage future upgrades.
* If all plugins are at the most recent version, there can be no conflicting same-package different-version issues.
* It encourages people to write human friendly code.

#### Wont this be a security issue?

No. A lot of people who use packages dont even read all of the code in said package. The same goes for plugins.

Only allowing bugfixes or minor version changes does not stop an attacker from releasing a malicious "minor upgrade" package. Relying on attackers to respect semantic versioning is bad.

And pinning your packages brings up the same issues discussed above.

#### Wont this mean my code will always be breaking?

No. If the people building the plugins do it correctly, minor changes will only cause minor bugs in your code. Major changes may cayse major bugs, but should be told to the user in advance, so they can prepare.

In addition, testing your code, combined with automatically updating plugins means you will be able to see the issues right away, instead of when you decide to upgrade.

And, by releasing smaller version updates more frequently, users of your code will find underlying issues sooner rather then later.

#### Wont this mean I will have to do more work?

Yes and no. Yes, you will have to fix the bugs that appear because of plugin updates. But, you will not have to make a lot of big/small changes, compared to upgrading from version to version manually (especially upgrading across many versions).
