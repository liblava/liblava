# Update all external modules to latest GIT_TAG in CPM.cmake

import json
import os
import requests

real_path = os.path.realpath(__file__)
dir_path = os.path.dirname(real_path)

with open(dir_path + '/list.json', 'r') as ext_file:
    ext = json.load(ext_file)
    with open('cmake/version.cmake', 'w') as version_file:
        output = []

        for module in ext:
            name = module['name']

            github = module['github']
            branch = module['branch']
            url = 'https://api.github.com/repos/' + github + '/branches/' + branch

            response = requests.get(url)
            info = json.loads(response.text)

            if 'commit' not in info:
                raise ValueError(info)

            if 'sha' not in info['commit']:
                raise ValueError(info)

            tag = info['commit']['sha']

            item = { 
                'name' : name,
                'github' : github,
                'branch' : branch,
                'tag' : tag
            }

            output.append(item)
            
            version_file.write('\nset(' + name + '_GITHUB ' + github + ')\n')
            version_file.write('set(' + name + '_TAG ' + tag + ')\n')
        
        print(json.dumps(output, indent=4))
