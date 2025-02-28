import json

def save_id(user_id):
    with open('user_id_cache.json', 'w') as file:
        json.dump({'user_id': user_id}, file)

def load_id():
    try:
        with open('user_id_cache.json', 'r') as file:
            data = json.load(file)
            return data['user_id']
    except FileNotFoundError:
        return None
