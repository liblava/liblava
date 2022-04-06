# Update all external modules to latest GIT_TAG in CPM.cmake

import json
import requests

with open("ext.json", "r") as ext_file:
    ext = json.load(ext_file)
    with open('cmake/version.cmake', 'w') as version_file:
        for module in ext:
            name = module
            print(name)

            github = ext[module]["github"]
            print(" - " + github)

            branch = ext[module]["branch"]
            print(" -- " + branch)

            url = "https://api.github.com/repos/" + github + "/branches/" + branch
            print(" --- " + url)
            
            response = requests.get(url)
            info = json.loads(response.text)

            tag = info["commit"]["sha"];
            print(" ---- " + tag)
            
            version_file.write("set(" + name + "_GITHUB " + github + ")\n")
            version_file.write("set(" + name + "_TAG " + tag + ")\n\n")
