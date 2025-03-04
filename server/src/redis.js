import Redis from "ioredis";

const redis = new Redis(process.env.REDIS_URL);

export async function fetchUser(userId) {
  try {
    const value = await redis.get(userId);
    return JSON.parse(value);
  } catch (error) {
    console.error("failed to fetch user:", error);
    return null;
  }
}

/** this is a very ugly function but making it better is not high priority right now */
export async function fetchUserByDiscordId(discordId) {
  try {
    const keys = await redis.keys("*");
    for (const key of keys) {
      const user = await fetchUser(key);
      if (user.discordId === discordId) return { userId: key, ...user };
    }
    return null;
  } catch (error) {
    console.error("failed to fetch user by discord id:", error);
    return null;
  }
}

export async function storeUser(userId, discordId, pairId) {
  try {
    await redis.set(userId, JSON.stringify({ discordId, pairId }));
    console.log(`stored user ${userId} in database`);
  } catch (error) {
    console.error("failed to store user:", error);
  }
}

export async function updateDiscordId(userId, discordId) {
  try {
    const user = await fetchUser(userId); // get current user object
    user.discordId = discordId; // update discord id
    await redis.set(userId, JSON.stringify(user)); // set it back
    console.log(`updated discord id for user ${userId}`);
  } catch (error) {
    console.error("failed to update discord id:", error);
  }
}

export async function updatePair(userId, pairId) {
  try {
    const user = await fetchUser(userId); // get current user object
    user.pairId = pairId; // update pair
    await redis.set(userId, JSON.stringify(user)); // set it back
    console.log(`updated pair for user ${userId}`);
  } catch (error) {
    console.error("failed to update pair:", error);
  }
}

export async function deletePairing(userId) {
  try {
    const user = await fetchUser(userId); // get current user object

    // unset pair's pairId
    if (user.pairId) {
      const pair = await fetchUser(user.pairId);
      pair.pairId = null;
      await redis.set(user.pairId, JSON.stringify(pair));
      console.log(`deleted pair for user ${user.paidId}`);
    }

    user.pairId = null;
    await redis.set(userId, JSON.stringify(user)); // unset pairId
    console.log(`deleted pair for user ${userId}`);
  } catch (error) {
    console.error("failed to delete pair:", error);
  }
}
