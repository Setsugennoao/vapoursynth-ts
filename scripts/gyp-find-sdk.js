const { execSync } = require('child_process')

let regKey = null

try {
    regKey = execSync('reg query "HKEY_CURRENT_USER\\SOFTWARE\\VapourSynth" /v Path', { encoding: 'utf-8'}).trim()
} catch {}


if (!regKey) {
    try {
        regKey = execSync('reg query "HKEY_LOCAL_MACHINE\\SOFTWARE\\VapourSynth" /v Path', { encoding: 'utf-8'}).trim()
    } catch {}
}

if (!regKey) process.exit(1)

const installationPath = regKey.split('\n')[1].split(' '.repeat(4))[3]

console.log(`${installationPath}\\sdk`)

// TODO support Linux and portable installations
