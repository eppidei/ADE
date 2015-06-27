function R=update_rotation_matrix_body2earth(yaw,pitch,roll)%[ψ,θ,φ]

R=transp(update_rotation_matrix_earth2body(yaw,pitch,roll));


end
