import requests
def get_artists_album(artist_id):
    url = f"https://api.discogs.com/artists/{artist_id}/releases"

    params = {
        "sort": "year",
        "sort_order": "asc",
    }

    response = requests.get(url, params=params)
    if response.status_code == 200:
        data = response.json()
        print(data)
        albums = []
        for release in data['releases']:
            albums.append(release['title'])
        return albums
    else:
        print(f"Error: {response.status_code}")
        return None

def main():
    artist_id = input("Enter the artist ID: ")
    albums = get_artists_album(artist_id)
    if albums:
        print(f"Albums by artist {artist_id}:")
        for album in albums:
            print(album)
    else:
        print("No albums found or an error occurred.")

if __name__ == "__main__":
    main()