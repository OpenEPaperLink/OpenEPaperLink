# Test notes

Most of the tests are now performed via GitHub Actions.

Unfortunately GitHub Actions provides only the latest macOS version, so
the multi-version macOS tests are still performed on Travis.

## GitHub Actions

### GitHub API endpoint

Programatic access to GitHub is done via the v3 API:

- <https://developer.github.com/v3/>

```sh
curl -i https://api.github.com/users/ilg-ul/orgs

curl -i https://api.github.com/repos/xpack-dev-tools/xxxx-xpack/releases

curl -v -X GET https://api.github.com/repos/xpack-dev-tools/xxxxx-xpack/hooks
```

For authenticated requests, preferably create a new token and pass it
via the environment.

- <https://developer.github.com/v3/#authentication>

### Trigger GitHub action

To trigger a GitHub action it is necessary to send an authenticated POST
at a specific URL:

- <https://developer.github.com/v3/repos/#create-a-repository-dispatch-event>

```sh
curl \
  --include \
  --header "Authorization: token ${GITHUB_API_DISPATCH_TOKEN}" \
  --header "Content-Type: application/json" \
  --header "Accept: application/vnd.github.everest-preview+json" \
  --data '{"event_type": "on-demand-test", "client_payload": {}}' \
  https://api.github.com/repos/xpack-dev-tools/xxxxx-xpack/dispatches
```

The request should return `HTTP/1.1 204 No Content`.

The repository should have an action with `on: repository_dispatch`.

## Travis-CI

### Enable Travis

To enable the travis tests:

- login to <https://travis-ci.com/> with the GitHub credentials
- in the user settings, select the **3rd Party xpack Dev Tools** organization
- enable the **meson-build-xpack** project
- in Setting, disable **Build pushed branches** and **Build pull requests**

### Test work flow

Travis tests are manually triggered, and use the files in the project repo,
so it is necessary to push latest changes in the `xpack-develop` branch
to GitHub.

The first step is one of the top `trigger-travis-*.sh` scripts,
which call a helper `trigger_travis()` function that posts a http
request via `curl` to the Travis API.

The test configurations are passed in the request JSON as an array of
jobs, that each execute several script lines in the context of a given OS.

These scripts call `tests/scripts/native-test.sh`, which can directly include
`tests/scripts/common-functions-source.sh` and call the test functions.

## Travis test results

The test results will be available at

- <https://travis-ci.com/github/xpack-dev-tools/xxxxx-xpack>
