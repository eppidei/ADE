function vec=quat_rotate(q,r)

quatr=[0,r(1),r(2),r(3)];
mult1=quatmultiply(q,quatr);
vec=quatmultiply(mult1,quatinv(q));


end