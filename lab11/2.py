from collections import defaultdict

import requests
import sys

URL = "https://api.discogs.com/artists/"

def get_artist_groups(artist_id):
    url = f"{URL}{artist_id}"
    response = requests.get(url)

    if response.status_code == 200:
        data = response.json()
        name = data.get('name')
        groups = data.get('groups', [])
        group_names = {group["id"]: group['name'] for group in groups}
        return name, group_names
    else:
        print(f"Error: {response.status_code}")
        return sys.exit(1)

def main():
    if len(sys.argv) < 2:
        print("Usage: python script.py <artist_id> ...")
        sys.exit(1)

    artist_ids = [int(aid) for aid in sys.argv[1:]]
    artist_groups= defaultdict(set)

    for id in artist_ids:
        name, groups = get_artist_groups(id)

        for g_id, g_name in groups.items():
            artist_groups[g_name].add(name)

    #groups with at least 2 members
    groups_2_members = {g: m for g, m in artist_groups.items() if len(m) >= 2}

    for x in sorted(groups_2_members.keys()):
        print(f"{x}: {', '.join(groups_2_members[x])}")

    sys.exit(0)


if __name__ == "__main__":
    main()